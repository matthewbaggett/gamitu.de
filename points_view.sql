DROP VIEW IF EXISTS points_view ;
CREATE VIEW points_view AS
SELECT 
  p.uid,
  p.pid,
  p.latitude,
  p.longitude,
  p.timestamp_ms,
  from_unixtime(p.timestamp_ms/1000) as time_human

FROM `points` p

