DROP VIEW IF EXISTS points_deltas ;
CREATE VIEW points_deltas AS
SELECT
  current.uid as uid,
  u.username as username,
  current.pid as pid1,
  current.latitude as lat_curr,
  current.longitude as long_curr,
  current.timestamp_ms as time_ms_curr,
  current.timestamp_ms / 1000 as time_s_curr,
  current.datetime as time_human_curr,
  previous.pid as pid_prev,
  previous.latitude as lat_prev,
  previous.longitude as long_prev,
  previous.timestamp_ms as time_ms_prev,
  previous.timestamp_ms / 1000 as time_s_prev,
  previous.datetime as time_human_prev,

  current.latitude - previous.latitude as lat_delta,
  current.longitude - previous.longitude as long_delta,
  current.datetime - previous.datetime as time_delta_ms,
  (current.datetime - previous.datetime) / 1000 as time_delta_s,

  CASE
    WHEN
      current.latitude - previous.latitude != 0 OR current.longitude - previous.longitude != 0
    THEN
      ( 3959 * acos( cos( radians(previous.latitude) )
               * cos( radians( current.latitude ) )
               * cos( radians( current.longitude ) - radians(previous.longitude) )
               + sin( radians( previous.latitude	) )
               * sin( radians( current.latitude ) ) ) )
    ELSE
      0
    END
	as distance_delta,

  CASE
    WHEN
      current.latitude - previous.latitude != 0 OR current.longitude - previous.longitude != 0
    THEN
      'Yes'
    ELSE
      'No'
    END
  as changed_state


FROM `points` current
JOIN `points` previous
  ON  current.pid_preceding = previous.pid
  AND current.uid = previous.uid
JOIN `users` u
  ON u.uid = current.uid
