-- phpMyAdmin SQL Dump
-- version 5.2.1deb3
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Jul 03, 2024 at 11:53 PM
-- Server version: 8.0.37-0ubuntu0.24.04.1
-- PHP Version: 8.3.6

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Database: `particlesensor`
--

-- --------------------------------------------------------

--
-- Table structure for table `pms5003`
--

CREATE TABLE `pms5003` (
  `datetime` datetime NOT NULL,
  `pm10_standard` int NOT NULL,
  `pm25_standard` int NOT NULL,
  `pm100_standard` int NOT NULL,
  `pm10_env` int NOT NULL,
  `pm25_env` int NOT NULL,
  `pm100_env` int NOT NULL,
  `particles_03um` int NOT NULL,
  `particles_05um` int NOT NULL,
  `particles_10um` int NOT NULL,
  `particles_25um` int NOT NULL,
  `particles_50um` int NOT NULL,
  `particles_100um` int NOT NULL,
  `analogvalue` int NOT NULL,
  `analogvolts` int NOT NULL,
  `batteryvolts` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
