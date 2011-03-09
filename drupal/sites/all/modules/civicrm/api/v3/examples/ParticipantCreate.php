<?php 

function participant_create_example(){
    $params = array(
    
                  'contact_id' 		=> '2',
                  'event_id' 		=> '1',
                  'status_id' 		=> '1',
                  'role_id' 		=> '1',
                  'register_date' 		=> '20070721',
                  'source' 		=> 'Online Event Registration: API Testing',
                  'event_level' 		=> 'Tenor',
                  'version' 		=> '3',
                  'participant_status_id' 		=> '1',
                  'participant_id' 		=> '4',

  );
  require_once 'api/api.php';
  $result = civicrm_api( 'participant','create',$params );

  return $result;
}

/*
 * Function returns array of result expected from previous function
 */
function participant_create_expectedresult(){

  $expectedResult = 
     array(
           'is_error' 		=> '0',
           'version' 		=> '3',
           'count' 		=> '1',
           'id' 		=> '4',
           'values' 		=> array(           '4' =>  array(
                      'id' => '4',
                      'contact_id' => '2',
                      'event_id' => '1',
                      'status_id' => '1',
                      'role_id' => '1',
                      'register_date' => '20070721',
                      'source' => 'Online Event Registration: API Testing',
                      'fee_level' => '',
                      'is_test' => '',
                      'is_pay_later' => '',
                      'fee_amount' => '',
                      'registered_by_id' => '',
                      'discount_id' => '',
                      'fee_currency' => '',
                      'campaign_id' => '',
           ),           ),
      );

  return $expectedResult  ;
}

