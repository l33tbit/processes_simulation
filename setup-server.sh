#!/bin/bash
###############################################################################
# SERVER BOOTSTRAP SCRIPT – Ubuntu Server (22.04+) for Dev/AI Startup
# Run as root. All configurable names are at the top – change them once.
#
# Features:
#   - Idempotent (safe to re-run)
#   - POSIX ACLs + SGID for team directories
#   - SSH hardening via drop-in config (does not touch sshd_config directly)
#   - fail2ban, Docker relocation, noatime, sudo restrictions
#   - Automatic deployment of SSH public keys for users
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# ---------------------------------------------------------------------------
# CONFIGURATION – CHANGE THESE VALUES FOR YOUR ENVIRONMENT
# ---------------------------------------------------------------------------

# ----- Base directory for all service data -----
BASE_DIR="/srv"

# ----- Groups (must be valid Linux group names) -----
GROUP_DEV="dev"
GROUP_AI="ai-eng"
GROUP_OPS="ops"
GROUP_MGMT="management"

# ----- Users: list of "username:groups:full_name" -----
# groups are comma-separated supplementary groups (docker group will be added automatically if Docker installed)
USERS=(
  "alanstack:${GROUP_DEV},docker:Alice Developer"
  "adamkernel:${GROUP_AI},docker:Bob ML Engineer"
  "leoroute:${GROUP_OPS},docker:Carol Sysadmin"
  "lucasnode:${GROUP_MGMT}:Dave Manager"
)

# ----- Temporary initial password for ALL new users (they must change at first login) -----
DEFAULT_TEMP_PASSWORD="TempPass123!"
# (Best practice: set to empty and rely on SSH keys + passwd -l; but script keeps fallback)

# ----- SSH public keys for users -----
# Format: "username:ssh-<type> <public-key> <comment>"
# You can list multiple keys per user by adding separate entries for the same username.
USER_SSH_KEYS=(
  "alanstack:ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAI... alice@workstation"
  "alanstack:ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQ... alice@laptop"
  "adamkernel:ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAI... bob@ml-server"
  "leoroute:ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAI... carol@ops-console"
  # Add more as needed – one line per key
)

# ----- SSH custom port -----
SSH_PORT=22

# ----- fail2ban settings -----
F2B_MAXRETRY=10
F2B_BANTIME=300   # seconds (1 hour)
F2B_FINDTIME=600   # seconds (10 minutes)

# ----- Docker data directory (moved from /var/lib/docker) -----
DOCKER_DATA_ROOT="${BASE_DIR}/docker/docker-root"

# ----- Directory hierarchy (relative to BASE_DIR) -----
DIR_PROJECTS_DEV="${BASE_DIR}/projects/dev"
DIR_PROJECTS_AI="${BASE_DIR}/projects/ai-eng"
DIR_SHARED_TEMPLATES="${BASE_DIR}/shared/templates"
DIR_SHARED_DOCS="${BASE_DIR}/shared/docs"
DIR_DOCKER_VOLUMES="${BASE_DIR}/docker/volumes"
DIR_DOCKER_COMPOSE="${BASE_DIR}/docker/compose"
DIR_DOCKER_CONFIGS="${BASE_DIR}/docker/configs"
DIR_AI_MODELS="${BASE_DIR}/ai-data/models"
DIR_AI_DATASETS="${BASE_DIR}/ai-data/datasets"
DIR_AI_CACHE="${BASE_DIR}/ai-data/cache"
DIR_BACKUPS="${BASE_DIR}/backups"

# ----- Log file (optional) -----
LOG_FILE="/var/log/bootstrap-$(date +%Y%m%d-%H%M%S).log"
SUMMARY_FILE="/root/bootstrap-summary-$(date +%Y%m%d-%H%M%S).txt"

# Redirect output to log + console
exec > >(tee -a "$LOG_FILE") 2>&1

# ---------------------------------------------------------------------------
# END OF CONFIGURATION – DO NOT EDIT BELOW UNLESS YOU KNOW WHAT YOU'RE DOING
# ---------------------------------------------------------------------------

# Redirect all output to log file + console
exec > >(tee -a "$LOG_FILE") 2>&1

echo "===== BOOTSTRAP STARTED at $(date) ====="

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------
log() {
  echo "[$(date +'%Y-%m-%d %H:%M:%S')] $*"
}

run_cmd() {
  log "RUN: $*"
  "$@"
}

# Idempotent group creation
ensure_group() {
  local group="$1"
  if getent group "$group" >/dev/null; then
    log "Group '$group' already exists, skipping."
  else
    run_cmd groupadd "$group"
    log "Group '$group' created."
  fi
}

# Idempotent user creation (does nothing if user exists)
create_user() {
  local username="$1"
  local groups="$2"
  local full_name="$3"
  local password="$4"

  if id "$username" &>/dev/null; then
    log "User '$username' already exists, skipping creation."
    # Optionally update supplementary groups
    run_cmd usermod -aG "$groups" "$username" 2>/dev/null || true
  else
    run_cmd useradd -m -s /bin/bash -G "$groups" -c "$full_name" "$username"
    echo "${username}:${password}" | run_cmd chpasswd
    run_cmd chage -d 0 "$username"
    log "User '$username' created with temporary password."
  fi
}

# Add SSH public key(s) for a user (idempotent)
add_ssh_keys_for_user() {
  local username="$1"
  local ssh_key="$2"   # full public key line

  local user_home
  user_home=$(eval echo "~$username")
  local ssh_dir="${user_home}/.ssh"
  local auth_keys="${ssh_dir}/authorized_keys"

  # Create .ssh directory with correct permissions if missing
  if [[ ! -d "$ssh_dir" ]]; then
    run_cmd mkdir -p "$ssh_dir"
    run_cmd chmod 700 "$ssh_dir"
    run_cmd chown "${username}:${username}" "$ssh_dir"
    log "Created .ssh directory for $username"
  fi

  # Create authorized_keys if missing
  if [[ ! -f "$auth_keys" ]]; then
    run_cmd touch "$auth_keys"
    run_cmd chmod 600 "$auth_keys"
    run_cmd chown "${username}:${username}" "$auth_keys"
    log "Created authorized_keys for $username"
  fi

  # Check if key already present (using the full line)
  if grep -Fxq "$ssh_key" "$auth_keys"; then
    log "SSH key for $username already present, skipping."
  else
    echo "$ssh_key" >> "$auth_keys"
    log "Added SSH key for $username"
    # Ensure permissions remain correct
    chmod 600 "$auth_keys"
    chown "${username}:${username}" "$auth_keys"
  fi
}

write_summary() {
  cat > "$SUMMARY_FILE" << EOF
========================================
BOOTSTRAP SUMMARY - $(date)
========================================

SERVER: $(hostname)
IP: $(hostname -I | awk '{print $1}')

--- USERS CREATED ---
$(for u in "${USERS[@]}"; do
  IFS=':' read -r username groups fullname <<< "$u"
  echo "  $username ($fullname) -> groups: $groups"
done)

--- SSH KEYS DEPLOYED ---
$(for entry in "${USER_SSH_KEYS[@]}"; do
  IFS=':' read -r username key <<< "$entry"
  echo "  $username: ${key:0:60}..."
done)

--- GROUPS ---
  dev: $GROUP_DEV
  ai-eng: $GROUP_AI
  ops: $GROUP_OPS
  management: $GROUP_MGMT

--- DIRECTORIES ---
  Base: $BASE_DIR
$(find "$BASE_DIR" -maxdepth 3 -type d 2>/dev/null | sed 's/^/  /')

--- SSH ---
  Port: $SSH_PORT
  Config: /etc/ssh/sshd_config.d/99-bootstrap.conf

--- DOCKER ---
  Data root: $DOCKER_DATA_ROOT
  Config: /etc/docker/daemon.json

--- FAIL2BAN ---
  Port: $SSH_PORT
  Max retry: $F2B_MAXRETRY
  Ban time: ${F2B_BANTIME}s

--- SUDO RULES ---
  File: /etc/sudoers.d/10-team-rules

--- LOG FILE ---
  $LOG_FILE

========================================
EOF
}

# Generate a random password (20 characters)
generate_password() {
  openssl rand -base64 15 | tr -d "=+/" | cut -c1-20
}

# ---------------------------------------------------------------------------
# STEP 0 – Pre-flight checks
# ---------------------------------------------------------------------------
if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root." >&2
  exit 1
fi

log "Pre-flight checks passed."

# ---------------------------------------------------------------------------
# STEP 1 – Install prerequisites
# ---------------------------------------------------------------------------
log "STEP 1: Installing prerequisite packages (acl, fail2ban, rsync, etc.)"
run_cmd apt-get update -qq
run_cmd apt-get install -y -qq acl fail2ban rsync tree
log "Prerequisites installed."

# ---------------------------------------------------------------------------
# STEP 2 – Create groups
# ---------------------------------------------------------------------------
log "STEP 2: Creating Linux groups"
for grp in "$GROUP_DEV" "$GROUP_AI" "$GROUP_OPS" "$GROUP_MGMT"; do
  ensure_group "$grp"
done
# Ensure docker group exists if Docker is installed (usually auto-created)
if command -v docker &>/dev/null; then
  ensure_group "docker"
fi
log "Groups created/verified."

# ---------------------------------------------------------------------------
# STEP 3 – Create user accounts
# ---------------------------------------------------------------------------
log "STEP 3: Creating user accounts"
for user_entry in "${USERS[@]}"; do
  IFS=':' read -r username groups full_name <<< "$user_entry"
  create_user "$username" "$groups" "$full_name" "$DEFAULT_TEMP_PASSWORD"
done

# Lock root password
passwd -l root
log "Root password locked."
log "User accounts created/updated."

# ---------------------------------------------------------------------------
# STEP 3b – Deploy SSH public keys
# ---------------------------------------------------------------------------
log "STEP 3b: Deploying SSH public keys"
if [[ ${#USER_SSH_KEYS[@]} -gt 0 ]]; then
  for key_entry in "${USER_SSH_KEYS[@]}"; do
    IFS=':' read -r username key <<< "$key_entry"
    if id "$username" &>/dev/null; then
      add_ssh_keys_for_user "$username" "$key"
    else
      log "WARNING: User $username does not exist – cannot add SSH key."
    fi
  done
else
  log "No SSH keys defined in USER_SSH_KEYS – users will need to add keys manually."
fi

# ---------------------------------------------------------------------------
# STEP 4 – Sudo rules (drop-in file)
# ---------------------------------------------------------------------------
log "STEP 4: Configuring sudo rules"
SUDO_FILE="/etc/sudoers.d/10-team-rules"
cat > "$SUDO_FILE" << EOF
# Docker management for dev & ai-eng (no full root)
%${GROUP_DEV} ALL=(root) NOPASSWD: /usr/bin/docker, /usr/bin/docker-compose
%${GROUP_AI} ALL=(root) NOPASSWD: /usr/bin/docker, /usr/bin/docker-compose

# Explicit deny list – dangerous commands even with sudo
%${GROUP_DEV} ALL=(root) !/usr/bin/docker system prune -a, !/sbin/reboot, !/sbin/shutdown, !/sbin/poweroff
%${GROUP_AI} ALL=(root) !/usr/bin/docker system prune -a, !/sbin/reboot, !/sbin/shutdown, !/sbin/poweroff

# Ops team: full administrative sudo
%${GROUP_OPS} ALL=(ALL) ALL

# Management: intentionally NO sudo rule (read-only by design)
EOF
chmod 0440 "$SUDO_FILE"
visudo -c && log "Sudo rules syntax OK." || { log "ERROR in sudoers file – aborting"; exit 1; }

# ---------------------------------------------------------------------------
# STEP 5 – Directory hierarchy
# ---------------------------------------------------------------------------
log "STEP 5: Building directory hierarchy under $BASE_DIR"
mkdir -p "$DIR_PROJECTS_DEV" "$DIR_PROJECTS_AI"
mkdir -p "$DIR_SHARED_TEMPLATES" "$DIR_SHARED_DOCS"
mkdir -p "$DIR_DOCKER_VOLUMES" "$DIR_DOCKER_COMPOSE" "$DIR_DOCKER_CONFIGS" "$DOCKER_DATA_ROOT"
mkdir -p "$DIR_AI_MODELS" "$DIR_AI_DATASETS" "$DIR_AI_CACHE"
mkdir -p "$DIR_BACKUPS"
log "Directory structure created."

# ---------------------------------------------------------------------------
# STEP 6 – Ownership, SGID, ACLs
# ---------------------------------------------------------------------------
log "STEP 6: Setting ownership, SGID bits, and POSIX ACLs"

apply_acls() {
  local dir="$1"
  local owner_group="$2"
  local acl_spec="$3"   # e.g., "group:dev:rwx,group:ai-eng:rwx"
  local sgid="${4:-true}"  # default to true

  run_cmd chown -R "root:${owner_group}" "$dir"
  if $sgid; then
    run_cmd chmod -R 2775 "$dir"
  else
    run_cmd chmod -R 770 "$dir"
  fi

  # Remove existing ACLs to avoid stacking (optional)
  setfacl -b -R "$dir" 2>/dev/null || true

  # Apply each ACL entry
  IFS=',' read -ra ENTRIES <<< "$acl_spec"
  for entry in "${ENTRIES[@]}"; do
    run_cmd setfacl -R -m "${entry}" "$dir"
    run_cmd setfacl -R -d -m "${entry}" "$dir"
  done
}

# /srv/projects/dev – full access for dev group
apply_acls "$DIR_PROJECTS_DEV" "$GROUP_DEV" "group:${GROUP_DEV}:rwx"

# /srv/projects/ai-eng – full access for ai-eng group
apply_acls "$DIR_PROJECTS_AI" "$GROUP_AI" "group:${GROUP_AI}:rwx"

# /srv/shared – read/write for both dev and ai-eng
apply_acls "$BASE_DIR/shared" "$GROUP_DEV" "group:${GROUP_DEV}:rwx,group:${GROUP_AI}:rwx"

# /srv/docker/compose & configs – write for dev and ai-eng
apply_acls "$DIR_DOCKER_COMPOSE" "$GROUP_DEV" "group:${GROUP_DEV}:rwx,group:${GROUP_AI}:rwx"
apply_acls "$DIR_DOCKER_CONFIGS" "$GROUP_DEV" "group:${GROUP_DEV}:rwx,group:${GROUP_AI}:rwx"

# /srv/ai-data – owner ai-eng, dev gets read-only
apply_acls "$BASE_DIR/ai-data" "$GROUP_AI" "group:${GROUP_AI}:rwx,group:${GROUP_DEV}:r-x"

# /srv/projects (top-level) – management gets read-only
setfacl -R -m "group:${GROUP_MGMT}:r-x" "$BASE_DIR/projects"
setfacl -R -d -m "group:${GROUP_MGMT}:r-x" "$BASE_DIR/projects"

# /srv/backups – ops only (no world access)
apply_acls "$DIR_BACKUPS" "$GROUP_OPS" "group:${GROUP_OPS}:rwx" "false"

log "ACLs applied."

# ---------------------------------------------------------------------------
# STEP 7 – noatime for HDD performance
# ---------------------------------------------------------------------------
log "STEP 7: Adding noatime mount option"
cp /etc/fstab "/etc/fstab.bak.$(date +%Y%m%d)"
if ! grep -q "noatime" /etc/fstab; then
  sed -i '/[[:space:]]\/[[:space:]]/ s/defaults/defaults,noatime/' /etc/fstab
  log "noatime added to /etc/fstab."
else
  log "noatime already present."
fi
mount -o remount / || true

# ---------------------------------------------------------------------------
# STEP 8 – Relocate Docker data-root
# ---------------------------------------------------------------------------
log "STEP 8: Relocating Docker data-root to $DOCKER_DATA_ROOT"
if systemctl is-active --quiet docker; then
  run_cmd systemctl stop docker
fi
mkdir -p /etc/docker
cat > /etc/docker/daemon.json << EOF
{
  "data-root": "${DOCKER_DATA_ROOT}",
  "storage-driver": "overlay2"
}
EOF

if [ -d /var/lib/docker ] && [ "$(ls -A /var/lib/docker 2>/dev/null)" ]; then
  run_cmd rsync -aP /var/lib/docker/ "${DOCKER_DATA_ROOT}/"
  mv /var/lib/docker /var/lib/docker.old
  log "Old Docker data backed up to /var/lib/docker.old"
fi
run_cmd systemctl start docker
log "Docker started with new data-root."

# ---------------------------------------------------------------------------
# STEP 9 – SSH hardening (using drop-in config)
# ---------------------------------------------------------------------------
log "STEP 9: Hardening SSH (port $SSH_PORT, key-only, no root login)"
SSHD_DROPIN="/etc/ssh/sshd_config.d/99-bootstrap.conf"
mkdir -p /etc/ssh/sshd_config.d

cat > "$SSHD_DROPIN" << EOF
# Custom SSH hardening – Bootstrap script
Port ${SSH_PORT}
PasswordAuthentication no
PermitEmptyPasswords no
PermitRootLogin no
PubkeyAuthentication yes
X11Forwarding no
MaxAuthTries 3
AllowGroups ${GROUP_DEV} ${GROUP_AI} ${GROUP_OPS} ${GROUP_MGMT}
EOF

sshd -t && log "SSH config syntax OK." || { log "ERROR in SSH config – restoring previous"; rm -f "$SSHD_DROPIN"; exit 1; }

# Ensure UFW is active and add new port (keep old port temporarily)
if command -v ufw &>/dev/null; then
  ufw allow ${SSH_PORT}/tcp
  log "UFW rule added for port ${SSH_PORT}."
fi

run_cmd systemctl restart sshd
log "SSH restarted with new settings."
echo ">>> IMPORTANT: Test SSH on port ${SSH_PORT} from a NEW terminal before disconnecting!"

# ---------------------------------------------------------------------------
# STEP 10 – fail2ban configuration
# ---------------------------------------------------------------------------
log "STEP 10: Setting up fail2ban for SSH"
cat > /etc/fail2ban/jail.local << EOF
[sshd]
enabled = true
port = ${SSH_PORT}
maxretry = ${F2B_MAXRETRY}
bantime = ${F2B_BANTIME}
findtime = ${F2B_FINDTIME}
EOF
run_cmd systemctl enable --now fail2ban
log "fail2ban started and enabled."

# ---------------------------------------------------------------------------
# Final summary
# ---------------------------------------------------------------------------
log "===== BOOTSTRAP COMPLETED at $(date) ====="
echo ""
echo "Directory structure:"
find "$BASE_DIR" -maxdepth 3 -type d | sort

echo ""
echo "NEXT STEPS (manual, not automated for safety):"
echo "  1. (Optional) If you added SSH keys, users can log in immediately."
echo "  2. After confirming SSH works on port ${SSH_PORT}, remove old rule: ufw delete allow 22/tcp"
echo "  3. Verify ACLs with: getfacl ${DIR_PROJECTS_DEV}"
echo "  4. All users must change their password on first login (enforced by chage)."
echo "  5. For users without pre‑deployed SSH keys, manually add their public key:"
echo "       mkdir -p /home/<user>/.ssh && echo 'key' > /home/<user>/.ssh/authorized_keys"
echo "       chmod 700 /home/<user>/.ssh && chmod 600 /home/<user>/.ssh/authorized_keys"
echo "       chown -R <user>:<user> /home/<user>/.ssh"
write_summary
echo "Summary saved to: $SUMMARY_FILE"
echo "Full log saved to: ${LOG_FILE}"