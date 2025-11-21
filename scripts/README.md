# Smart Classroom Watch - Utility Scripts

Collection of utility scripts for setup, deployment, and maintenance.

## Scripts

### setup.sh
Sets up the development environment
```bash
./scripts/setup.sh
```

### deploy.sh
Deploys to production/staging server
```bash
# Deploy to production
./scripts/deploy.sh production main

# Deploy to staging
./scripts/deploy.sh staging develop
```

### backup.sh
Creates backup of database and files
```bash
# Manual backup
./scripts/backup.sh

# Scheduled backup (cron)
0 2 * * * /path/to/scripts/backup.sh
```

### restore.sh
Restores from backup
```bash
./scripts/restore.sh backups/smart-classroom-backup-20250121_140530.tar.gz
```

## Environment Variables

### Deploy Script
- `SERVER_USER`: SSH user (default: ubuntu)
- `SERVER_HOST`: Server hostname
- `APP_DIR`: Application directory on server
- `WEBHOOK_URL`: Notification webhook (optional)

### Backup Script
- `BACKUP_DIR`: Backup directory (default: ./backups)
- `RETENTION_DAYS`: Days to keep backups (default: 30)
- `AWS_S3_BUCKET`: S3 bucket for cloud backup (optional)

## Automation

### Cron Jobs
```bash
# Daily backup at 2 AM
0 2 * * * /path/to/scripts/backup.sh

# Weekly cleanup
0 3 * * 0 find /path/to/backups -mtime +30 -delete
```

## License
All utility scripts are complete! ✅
📋 Summary:
✅ setup.sh - Complete environment setup
✅ deploy.sh - Production deployment with health checks
✅ backup.sh - Automated backups with cloud storage
✅ restore.sh - Easy restoration from backups

## 🚀 Quick Usage:
# Setup
chmod +x scripts/*.sh
./scripts/setup.sh

# Deploy
./scripts/deploy.sh production main

# Backup
./scripts/backup.sh

# Restore
./scripts/restore.sh backups/smart-classroom-backup-*.tar.gz
MIT

