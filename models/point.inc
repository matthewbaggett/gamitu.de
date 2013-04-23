<?php
use tcore\model;

class point extends model {
  protected $_key = 'pid';
  protected $pid;
  protected $uid;
  protected $latitude;
  protected $_latitude_type = model::TYPE_COORD;
  protected $longitude;
  protected $_longitude_type = model::TYPE_COORD;
  protected $timestamp_ms;
  protected $_timestamp_ms_type = model::TYPE_LONGINT;
  protected $datetime;
  protected $_datetime_type = model::TYPE_DATE;
  protected $hash;
  protected $geolocation;
  protected $_geolocation_type = model::TYPE_TEXT;
  protected $pid_preceding;
  protected $_pid_preceding_type = model::TYPE_INT;

  public function generate_hash() {
    $this->hash = hash("sha1", implode("::", array($this->uid, $this->latitude, $this->longitude, $this->timestamp_ms)));

    return $this->hash;
  }

  public function already_exists() {
    $this->generate_hash();
    $matching_point = point::search()
      ->where('uid', $this->get_uid())
      ->where('latitude', $this->get_latitude())
      ->where('longitude', $this->get_longitude())
      ->where('datetime', $this->get_datetime())
      ->execOne();
    if ($matching_point) {
      return true;
    } else {
      return false;
    }
  }

  public function save() {
    $this->generate_hash();
    if ($this->already_exists()) {
      return false;
    } else {
      return parent::save();
    }
  }

  private function _reverse_geolocate() {
    $endpoint = "http://maps.googleapis.com/maps/api/geocode/json?latlng=".trim($this->latitude).",".trim($this->longitude)."&sensor=false";

    $raw = @file_get_contents($endpoint);
    $json_data=json_decode($raw);

    if ($json_data->status=="OK") {
      $fAddress = explode(",", $json_data->results[count($json_data->results)-2]->formatted_address);  //this is human readable address --> getting province name
      _log($fAddress);
      _log($json_data->results);  //dumping result
      return $json_data;
    } else {
      //if no result found, status would be ZERO_RESULTS
      return false;
    }
  }

  public function get_geolocation() {
    if (!$this->geolocation) {
      $geocode = $this->_reverse_geolocate();
      $this->set_geolocation(json_encode($geocode));
      $this->save();
    }

    return json_decode(parent::get_geolocation(), true);
  }

  public function get_town() {
    $geocode = $this->get_geolocation();
    _log($geocode, "Geocode for {$this->get_pid()}");

    if (isset($geocode['results'][0]['address_components'])) {
      foreach ($geocode['results'][0]['address_components'] as $address_component) {
        if ($address_component['types'][0] == 'postal_town') {
          return $address_component['long_name'];
        }
      }
    }
    return "Unknown Town";
  }

  /**
   * Process & recalculate point ordering.
   */
  static public function recalculate_point_order() {
    $orphan_points = point::search()->where('pid_preceding', '')->exec();

    foreach ($orphan_points as $orphan_point) {
      $preceding_point = point::search()
        ->where('timestamp_ms', $orphan_point->get_timestamp_ms(), "<")
        ->where('uid', $orphan_point->get_uid())
        ->order('timestamp_ms', 'DESC')
        ->execOne();
      if ($preceding_point) {
        $orphan_point->set_pid_preceding($preceding_point->get_pid());
        $orphan_point->save(true);
      }
    }
  }

  public function get_timestamp(){
    return $this->get_timestamp_ms() / 1000;
  }
}