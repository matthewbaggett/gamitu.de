DROP VIEW IF EXISTS points_deltas ;
CREATE VIEW points_deltas AS
SELECT
  p_a.uid as uid,
  p_a.pid as pid1,
  p_a.latitude as lat_1,
  p_a.longitude as long_1,
  p_a.timestamp_ms as time_ms_1,
  p_a.timestamp_ms / 1000 as time_s_1,
  p_a.datetime as time_human_1,
  p_b.pid as pid_2,
  p_b.latitude as lat_2,
  p_b.longitude as long_2,
  p_b.timestamp_ms as time_ms_2,
  p_b.timestamp_ms / 1000 as time_s_2,
  p_b.datetime as time_human_2,

  p_a.latitude - p_b.latitude as lat_delta,
  p_a.longitude - p_b.longitude as long_delta,
  p_a.datetime - p_b.datetime as time_delta_ms,
  (p_a.datetime - p_b.datetime) / 1000 as time_delta_s,

  ( 3959 * acos( cos( radians(p_b.latitude) )
               * cos( radians( p_a.latitude ) )
               * cos( radians( p_a.longitude ) - radians(p_b.longitude) )
               + sin( radians( p_b.latitude	) )
               * sin( radians( p_a.latitude ) ) ) )
	as distance_delta

FROM `points` p_a
JOIN `points` p_b
  ON p_a.pid_preceding = p_b.pid AND p_a.uid = p_b.uid
