<?php

/**
 * @file
 * API documentation.
 */

/**
 * Triggered when an IP embargo is set on an object.
 *
 * @param string $pid
 *   The PID of the object for which an embargo was set.
 * @param array $parameters
 *   An array of parameters that were sent to the database to describe this
 *   object's embargo, including:
 *   - lid: The ID of the list the embargo was added to.
 *   - expiry: An integer representing the expiry timestamp, or NULL if the
 *     embargo does not expire.
 */
function hook_islandora_ip_embargo_set($pid, array $parameters) {
  $object = islandora_object_load($pid);
  if ($object) {
    $object->label .= " (EMBARGOED)";
    if ($parameters['expiry']) {
      $object->state = 'I';
    }
  }
}

/**
 * Triggered when an IP embargo is lifted from an object.
 *
 * @param string $pid
 *   The PID of the object whose embargo was lifted.
 */
function hook_islandora_ip_embargo_lifted($pid) {
  global $user;
  // Assumes a theoretical "islandora_ip_embargo_notifier" module that invokes
  // hook_mail() and can parse the given parameters.
  drupal_mail('islandora_ip_embargo_notifier', 'lifted', $user->mail, user_preferred_language($user), array(
    'pid' => $pid,
  ));
}
