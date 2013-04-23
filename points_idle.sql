DROP VIEW IF EXISTS points_idle ;
CREATE VIEW points_idle AS
SELECT
 CONCAT(`latitude`, ', ', `longitude`) as latlong,
  `latitude`,
  `longitude`,
  COUNT(*) AS `frequency`
FROM `points`
GROUP BY latlong
ORDER BY `frequency` DESC