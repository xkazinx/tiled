-- The game's database (MySQL 8): the accounts. Idempotent: run on every install.
--   mysql tiled < schema.sql
-- `password`: a 32-character random salt, then SHA-256 (hex) of the salt and the
-- password (Sql::CreateAccount / CheckLogin).
CREATE TABLE IF NOT EXISTS `players` (
	`uid` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`username` VARCHAR(32) NOT NULL,
	`password` CHAR(96) NOT NULL,
	`created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY (`uid`),
	UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
