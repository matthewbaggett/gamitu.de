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
      ->where('hash', $this->hash)
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
    $bc = new BaseController();
    $g_client = $bc->get_google_client();
    $g_maps_geocoding = new Eden_Google_Maps_Geocoding($g_client);
    $g_maps_geocoding->setLatLng($this->latitude, $this->longitude);
    $g_maps_geocoding->setSensor('true');
    $geocode = $g_maps_geocoding->getResponse();
    return $geocode;
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
}