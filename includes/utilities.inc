<?php
/**
 * @file
 * Holds helper functions for islandora_ip_embargo.
 */

/**
 * Get all lists for table.
 *
 * This is not a general function it is used for pagination.
 *
 * @return SelectQuery
 *   The query return.
 */
function islandora_ip_embargo_get_lists() {
  $query = db_select("islandora_ip_embargo_lists");
  $query = $query->fields('islandora_ip_embargo_lists');
  $query = $query->extend('TableSort');
  $query = $query->extend('PagerDefault');
  $query = $query->limit(10);

  return $query->execute();
}

/**
 * Create a list.
 *
 * @param string $name
 *   The name of the new list.
 */
function islandora_ip_embargo_add_list($name) {
  db_insert('islandora_ip_embargo_lists')
  ->fields(array('name' => $name))
  ->execute();
}

/**
 * Edit a list.
 *
 * @param string $name
 *   The name of the list.
 * @param int $list_identifier
 *   The identifier of the list.
 */
function islandora_ip_embargo_edit_list($name, $list_identifier) {
  db_update('islandora_ip_embargo_lists')
  ->fields(array('name' => $name))
  ->condition('lid', $list_identifier)
  ->execute();
}

/**
 * Remove a list.
 *
 * @param int $list_identifier
 *   The identifier of the list to delete.
 */
function islandora_ip_embargo_remove_list($list_identifier) {
  db_delete('islandora_ip_embargo_lists')
  ->condition('lid', $list_identifier)
  ->execute();
}

/**
 * Get the name of a list.
 *
 * @param int $list_identifier
 *   The identifier of the list to delete.
 *
 * @return string
 *   The human readable name of the list.
 */
function islandora_ip_embargo_get_list_name($list_identifier) {
  $list = db_select('islandora_ip_embargo_lists')
  ->fields('islandora_ip_embargo_lists')
  ->condition('lid', $list_identifier)
  ->execute();
  $data = $list->fetchObject();

  return $data->name;
}

/**
 * Get a list's information.
 *
 * This is not a general function it is used for pagination.
 *
 * @param int $list_identifier
 *   The identifier of the list.
 *
 * @return SelectQuery
 *   The query return.
 */
function islandora_ip_embargo_get_lists_information($list_identifier) {
  $query = db_select("islandora_ip_embargo_lists", 'lists');
  $query->join('islandora_ip_embargo_ip_ranges', 'ranges', 'lists.lid = ranges.lid');
  $query = $query->fields('ranges');
  $query = $query->fields('lists');
  $query = $query->condition('lists.lid', $list_identifier);
  $query = $query->extend('TableSort');
  $query = $query->extend('PagerDefault');
  $query = $query->limit(10);

  return $query->execute();
}

/**
 * Remove an IP address range.
 *
 * @param int $range_identifier
 *   The identifier of the range to delete.
 */
function islandora_ip_embargo_remove_range($range_identifier) {
  db_delete('islandora_ip_embargo_ip_ranges')
  ->condition('rid', $range_identifier)
  ->execute();
}

/**
 * Create an IP address range on a list.
 *
 * @param int $list_identifier
 *   The identifier of the list.
 * @param string $low_end
 *   The low end of the IP range.
 * @param string $high_end
 *   The high end of the IP range.
 */
function islandora_ip_embargo_add_ip_range($list_identifier, $low_end, $high_end) {
  db_insert('islandora_ip_embargo_ip_ranges')
  ->fields(array(
    'low_end' => $low_end,
    'high_end' => $high_end,
    'lid' => $list_identifier,
  ))
  ->execute();
}

/**
 * Get an object's embargo information.
 *
 * @param string $islandora_object_id
 *   The PID of the object to get the embargo for.
 *
 * @return SelectQuery
 *   The query return.
 */
function islandora_ip_embargo_get_embargo($islandora_object_id) {
  $embargoes = db_select('islandora_ip_embargo_embargoes')
  ->fields('islandora_ip_embargo_embargoes')
  ->condition('pid', $islandora_object_id)
  ->execute();

  return $embargoes;
}

/**
 * Gets embargoes associated with a list.
 *
 * @param string $list_identifier
 *   The id of the list to find embargoes for.
 *
 * @return SelectQuery
 *   The query return.
 */
function islandora_ip_embargo_get_embargos($list_identifier) {
  $embargoes = db_select('islandora_ip_embargo_embargoes')
  ->fields('islandora_ip_embargo_embargoes')
  ->condition('lid', $list_identifier)
  ->execute();

  return $embargoes;
}

/**
 * Update an object's embargo information.
 *
 * @param string $islandora_object_id
 *   The PID of the object to update the embargo for.
 * @param int $list_id
 *   The IP address range list to use for the embargo.
 * @param int $expiry
 *   The timestamp to stop the embargo at.
 */
function islandora_ip_embargo_update_embargo($islandora_object_id, $list_id, $expiry = NULL) {
  db_update('islandora_ip_embargo_embargoes')
  ->fields(array(
    'pid' => $islandora_object_id,
    'lid' => $list_id,
    'expiry' => $expiry,
  ))
  ->condition('pid', $islandora_object_id)
  ->execute();
}

/**
 * Set an object's embargo information.
 *
 * @param string $islandora_object_id
 *   The PID of the object to set the embargo for.
 * @param int $list_id
 *   The IP address range list to use for the embargo.
 * @param int $expiry
 *   The timestamp to stop the embargo at.
 */
function islandora_ip_embargo_set_embargo($islandora_object_id, $list_id, $expiry = NULL) {
  db_insert('islandora_ip_embargo_embargoes')
  ->fields(array(
    'pid' => $islandora_object_id,
    'lid' => $list_id,
    'expiry' => $expiry,
  ))
  ->execute();
}

/**
 * Remove an object's embargo information.
 *
 * @param string $embargo_identifier
 *   The UID of the object to remove the embargo from.
 * @param bool $is_pid
 *   Boolean defining whether the $embargo_identifier is an object PID.
 *
 *   If set to FALSE, $embargo_identifier will be treated as an EID.
 */
function islandora_ip_embargo_remove_embargo($embargo_identifier, $is_pid = FALSE) {
  if (!$is_pid) {
    db_delete('islandora_ip_embargo_embargoes')
    ->condition('eid', $embargo_identifier)
    ->execute();
  }
  else {
    db_delete('islandora_ip_embargo_embargoes')
    ->condition('pid', $embargo_identifier)
    ->execute();
  }
}

/**
 * Will check if the current user's request IP is in an indicated range.
 *
 * @param string $low
 *   The low end bound of the IP address range.
 * @param string $high
 *   The high end bound of the IP address range.
 */
function islandora_ip_embargo_ip_in_range($low, $high) {
  $comparable_address = ip2long(ip_address());
  $comparable_high = ip2long($high);
  $comparable_low = ip2long($low);
  if ($comparable_address >= $comparable_low && $comparable_address <= $comparable_high) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Remove all embargoes if the expiry timestamp has passed.
 *
 * @param int $time_stamp
 *   The timestamp to judge embargo expiries by.
 */
function islandora_ip_embargo_cron_cleanup($time_stamp) {
  $num_deleted = db_delete('islandora_ip_embargo_embargoes')
  ->condition('expiry', $time_stamp, '<=')
  ->execute();

  watchdog(
    'Cron Job End',
    "The ip embargo cron job has ended, $num_deleted embargos have been removed."
  );
}
