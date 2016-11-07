<?php

function islandora_ip_embargo_drush_command(){
    $items = array();
    $items['islandora-ip-embargo-addRange'] = array(
        'description' => 'Add a network address range to the embargo list.',
        'options' => array(
            'low' => array(
                'description' => 'First address in the IP range (e.g, "172.16.0.0").',
                'required' => 'TRUE',
            ),
            'high' => array(
                'description' => 'Last address in the IP range (e.g., "172.16.255.255").',
                'required' => 'TRUE',
            ),
            'list' => array(
                'description' => 'Name of the list to add this range to (will be created if necessary).',
                'required' => 'TRUE',
            ),
        ),
        'aliases' => array('iipeadd'),
    );
    $items['islandora-ip-embargo-list-lists'] = array(
        'description' => 'Returns all the network lists defined on the system.',
        'aliases' => array('iipell'),
    );
    return $items;
}


function drush_islandora_ip_embargo_addRange(){
    module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
    $params = array(
        'low' => drush_get_option('low'),
        'high' => drush_get_option('high'),
        'list' => drush_get_option('list'),
    );

    $low  = $params['low'];
    $high = $params['high'];
    // check that input vars are valid IPs
    foreach(array($low, $high) as $ip){
        if (filter_var($ip, FILTER_VALIDATE_IP) === false) {
            //drupal_set_message(t("!!! %i is NOT a valid IP address", array('%i' => $ip)), 'error');
            drush_set_error("Invalid IP: $ip");
            return;
        }
    }

    // set up lists vars
    $target_list = $params['list'];
    $target_list_id = false;
    $lists = islandora_ip_embargo_get_lists();

    foreach($lists as $list){
        if($target_list == $list->name){
            $target_list_id = $list->lid;
            if(is_duplicate_range($list->lid, $low, $high)){
                return;
            }
            break;
        }
    }
    if(!$target_list_id){
        islandora_ip_embargo_add_list($target_list);
        $target_list_id = islandora_ip_embargo_get_list_object_by_name($target_list)->lid;
        drupal_set_message(t('Created list %l', array('%l'=> $target_list)));
    }

    islandora_ip_embargo_add_ip_range($target_list_id, $low, $high);
}

function is_duplicate_range($lid, $low, $high){
    foreach(islandora_ip_embargo_get_ranges($lid) as $range){
        if($range->low_end == $low && $range->high_end == $high){
            drush_set_error("Duplicate IP range.");
            return true;
        }
    }
}


function drush_islandora_ip_embargo_list_lists(){
    module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');

    $lists = islandora_ip_embargo_get_lists();
    printf("Network lists\n==============\n");
    foreach($lists as $list){
        printf("%s\n", $list->name);
    }
}