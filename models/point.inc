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
	
	public function generate_hash(){
		$this->set_hash(hash("sha1", implode("::",array($this->uid,$this->latitude,$this->longitude,$this->timestamp_ms))));
	}
	
	public function save(){
		$this->generate_hash();
		$existing_match = self::factory()
			->search()
			->where('hash', $this->get_hash())
			->execOne();
		
		if($existing_match){
			return FALSE;
		}else{
			parent::save();
			return TRUE;
		}
	}
	
	private function _reverse_geolocate(){
		$geocode = eden('google')
			->maps(user::get_current_user()->get_access_token())
			->geocoding()
				->setLatLng($this->latitude,$this->longitude)
				->setSensor('true')
				->getResponse();
		return $geocode;
	}
	
	public function get_town(){
		$geocode = $this->_reverse_geolocate();	
		foreach($geocode['results'][0]['address_components'] as $address_component){
			if($address_component['types'][0] == 'postal_town'){
				return $address_component['types'][0]['long_name'];
			}
		}
		return "Unknown Town";
	}
}