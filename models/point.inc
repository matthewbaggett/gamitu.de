<?php 
use tcore\model;

class point extends model{
	protected $_key = 'pid';
	protected $pid;
	protected $uid;
	protected $latitude;
	protected $_latitude_type = model::TYPE_COORD;
	protected $longitude;
	protected $_longitude_type = model::TYPE_COORD;
	protected $timestamp_ms;
	protected $hash;
	protected $geolocation;
	protected $_geolocation_type = model::TYPE_BLOB;
	protected $pid_preceding;
	protected $_pid_preceding_type = model::TYPE_INT;
	
	public function generate_hash(){
        $hash = hash("sha1", implode("::",array($this->uid,$this->latitude,$this->longitude,$this->timestamp_ms)));
		$this->set_hash($hash);
        return $hash;
	}
    
    /**
     * Override the default _is_dirty() to decide if the hash matches (IE: Not dirty) first.
     */
    protected function _is_dirty(){
    	$hash = $this->generate_hash();
        $existing_match = self::factory()
    		->search()
			->where('hash', $hash)
			->execOne();
        echo "Hash: {$hash}\n";
        if($existing_match){
            return FALSE;
        }
        return parent::_is_dirty();
	}
	
	private function _reverse_geolocate(){
		$geocode = eden('google')
			->maps(user::get_current_user()->get_access_token())
			->geocoding()
				->setLatLng($this->latitude, $this->longitude)
				->setSensor('true')
				->getResponse();
		return $geocode;
	}
	
	public function get_geolocation(){
		if(!$this->geolocation){
			$geocode = $this->_reverse_geolocate();
			$this->set_geolocation(json_encode($geocode));
			$this->save();
		}
		return json_decode(parent::get_geolocation(), true);
	}
	
	public function get_town(){
		$geocode = $this->get_geolocation();
		foreach($geocode['results'][0]['address_components'] as $address_component){
			if($address_component['types'][0] == 'postal_town'){
				return $address_component['long_name'];
			}
		}
		return "Unknown Town";
	}
	
	/**
	 * Process & recalculate point ordering.
	 */
	static public function recalculate_point_order(){
		$orphan_points = point::search()->where('pid_preceding', '')->exec();
		
		foreach($orphan_points as $orphan_point){
			$preceding_point = point::search()
				->where('timestamp_ms', $orphan_point->get_timestamp_ms(), "<")
				->where('uid', $orphan_point->get_uid())
				->order('timestamp_ms', 'DESC')
				->execOne();
			if($preceding_point){
				$orphan_point->set_pid_preceding($preceding_point->get_pid());
				$orphan_point->save(true);
			}
		}
	}
}