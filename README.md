# Islandora IP Embargo[![Build Status](https://travis-ci.org/Islandora-Labs/islandora_ip_embargo.png?branch=7.x)](https://travis-ci.org/Islandora-labs/islandora_ip_embargo)

## Introduction

A Drupal based approach to embargoing content based on IP ranges.

## Requirements

The following Drupal modules are required:

* [Islandora](https://github.com/islandora/islandora)
* [Islandora Access Override](https://github.com/discoverygarden/islandora_access_override)

### Optional Requirements

The following Drupal modules, while not required, add more functionality to this module:

* [Rules](https://drupal.org/project/rules)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

### Drupal cron configuration

Drupal cron should be minimally configured to run once a day to clean out expired embargoes.

### Network address lists

Go to `yoursite.com/admin/islandora/tools/ip_embargo` to manage network address lists. At least one list is required in order to use IP embargo, because these lists are what will appear in the 'IP address range list' drop-down in the 'IP Embargo' tab for any given object, and an IP embargo can't be set without an IP address range list.

#### Adding a list

1. Click 'Add network list'.
2. Enter a name for the list and click 'Create list'.
3. Now an [IP address range can be added](#adding-an-ip-address-range) if desired.

NOTE: Without an IP address range, no IP addresses will be able to access objects embargoed with that list.

#### Deleting a list

NOTE: If the list to be deleted has an IP address range, [the IP address ranges in that list must be deleted](#deleting-an-ip-address-range) before the list can be deleted.

1. Select a network list.
2. Click the 'Delete' below the table of network lists.

#### Adding an IP address range

IP address ranges can be specified to be allowed to access embargoed items that use a certain list by adding an IP address range to that list. Without an IP address range, no IP addresses will be able to access objects embargoed with that list.

1. Click on the list to be edited.
2. Click 'Add IP address range'.
3. Enter the IP address range of the addresses to be ALLOWED to access any object that gets embargoed with this list.

#### Deleting an IP address range

1. Click on the list to be edited.
2. Select the range(s) to delete.
3. Click the 'Delete ranges' button below the table of ranges.

### Module settings

Islandora IP Embargo settings can be edited in a form located at `yoursite.com/admin/islandora/tools/ip_embargo/misc`. This page allows an admin to set a custom redirect page for embargoed objects, set the number of days before an embargo alert, set the overlay text for embargoed items, and set the colour of the overlay text on embargoed items.

## Usage

### Embargoing objects

1. Go to the object to be embargoed; this object can also be a collection.
2. Click the 'IP Embargo' tab, fill out the form, and click 'Set Embargo'.
3. Now this object will appear in the [managing embargoed objects form](#manage-embargoed-objects)

### Manage embargoed objects

Go to `yoursite.com/admin/islandora/tools/ip_embargo/manage`. From there, objects can be selected and removed from their embargo by clicking the 'Delete' button. This page is also useful for finding embargoed objects to edit or examine by clicking on the desired object in the table.

## Troubleshooting

This module is not integrated with the Islandora Embargo module.
This module breaks islandora_xml_site_map with its redirects. 

## Maintainers/Sponsors

Current maintainers:

* [William Panting](https://github.com/willtp87)
* [discoverygarden](https://github.com/discoverygarden)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
