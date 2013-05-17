DROP VIEW IF EXISTS points_view ;
CREATE VIEW points_view AS
SELECT
  pd.uid as uid,
  pd.username as username,
  pd.pid1 as pid,

  pd.time_ms_curr / 1 as time_ms,
  pd.time_ms_curr / 1000 as time_s,
  pd.time_human_curr as time_human,


  pd.lat_delta as lat_delta,
  pd.long_delta as long_delta,
  pd.time_delta_ms as time_delta_ms,
  pd.time_delta_s as time_delta_s,

  pd.distance_delta as distance_delta,
  pd.changed_state as changed_state

FROM `points_deltas` pd
ORDER BY time_s DESC

