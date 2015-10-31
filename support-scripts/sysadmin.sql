-- MySQL dump 10.13  Distrib 5.6.23-72.1, for Linux (x86_64)
--
-- Host: localhost    Database: sysadmin
-- ------------------------------------------------------
-- Server version	5.6.23-72.1-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_xtra_backup_metadata`
--
create database sysadmin default character set utf8 default collate utf8_general_ci;

use sysadmin;

DROP TABLE IF EXISTS `t_xtra_backup_metadata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_xtra_backup_metadata` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `metadata_type` varchar(256) NOT NULL DEFAULT 'full-backuped',
  `metadata_from_lsn` bigint(20) NOT NULL,
  `metadata_to_lsn` bigint(20) NOT NULL,
  `metadata_last_lsn` bigint(20) NOT NULL,
  `xtrabackup_compact` int(11) DEFAULT NULL,
  `is_deleted` int(11) NOT NULL DEFAULT '0',
  `created_datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `base_backup_directory` varchar(512) NOT NULL DEFAULT '/Database/Backup',
  `backup_directory_name` varchar(512) NOT NULL,
  `baseon_backup` varchar(256) DEFAULT NULL,
  `extra_lsndir` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-10-20  2:50:55
