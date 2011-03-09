<?
require_once 'api/v3/utils.php';

/**
 *  returns the list of all the entities that you can manipulate via the api. The entity of this API call is the entity, that isn't a real civicrm entity as in something stored in the DB, but an abstract meta object. My head is going to explode. In a meta way.
 */
function civicrm_entity_get ($params) {
  _civicrm_initialize( true );
   $entities = array ();
   $iterator = new DirectoryIterator(dirname(__FILE__));
   foreach ($iterator as $fileinfo) {
     $file = $fileinfo->getFilename();
     $parts = explode(".", $file);  
     if (end($parts) == "php" &&  $file != "utils.php" ) {
       $entities [] = substr ($file, 0, -4); // without the ".php"
     }
  }
  sort($entities);
  return civicrm_create_success ($entities);
}

/**
 *  Placeholder function. This should never be called, as it doesn't have any meaning
 */
function civicrm_entity_create ($params) {
  return civicrm_create_error ("Creating a new entity means modifying the source code of civiCRM.");
}

/**
 *  Placeholder function. This should never be called, as it doesn't have any meaning
 */
function civicrm_entity_delete ($params) {
  return civicrm_create_error ("Deleting an entity means modifying the source code of civiCRM.");
}

/**
 *  Placeholder function. This should never be called, as it doesn't have any meaning
 */
function civicrm_entity_getfields ($params) {
  return civicrm_create_error ("entity_get only returns the list of entities you can access from the API, no fields");
}

?>
