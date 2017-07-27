# Islandora IP Embargo[![Build Status](https://travis-ci.org/Islandora/islandora_ip_embargo.png?branch=7.x)](https://travis-ci.org/Islandora/islandora_ip_embargo)
CI Server:
http://jenkins.discoverygarden.ca

A Drupal based approach to embargoing content based on IP ranges.
This module is not integrated with the Islandora Embargo module.
This module breaks islandora_xml_site_map with its redirects. 

## Requirements

The following Drupal modules are required:

* [Islandora](https://github.com/islandora/islandora)
* [Rules](https://drupal.org/project/rules) (While not a dependency, more functionality can be found by installing this module.)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

### Drupal cron configuration

Drupal cron should be configured to run once a day to clean out expired embargoes.

### Network address lists

Go to `yoursite.com/admin/islandora/tools/ip_embargo` to manage network address lists. You need at least one list to use IP embargo, because these lists are what will appear in the 'IP address range list' drop-down in the 'IP Embargo' tab for any given object, and you can't set an IP embargo without an IP address range list.

#### Adding a list

1. Click 'Add network list'.
2. Enter a name for your list and click 'Create list'.
3. Now you can [add an IP address range](#adding-an-ip-address-range) if desired.

NOTE: Without an IP address range, no IP addresses will be able to access objects embargoed with that list.

#### Deleting a list

NOTE: If the list you want to delete has an IP address range, you need to [delete all IP address ranges in that list](#deleting-an-ip-address-range) before you can delete the list.

1. Select a network list.
2. Click the 'Delete' below the table of network lists.

#### Adding an IP address range

You can choose to allow certain IP address ranges to access embargoed items that use a certain list by adding an IP address range to that list. Without an IP address range, no IP addresses will be able to access objects embargoed with that list.

1. Click on the list you want to edit.
2. Click 'Add IP address range'.
3. Enter the IP address range of the addresses you want to ALLOW access for any object that gets embargoed with this list.

#### Deleting an IP address range

1. Click on the list you want to edit.
2. Select the range(s) you want to delete.
3. Click the 'Delete ranges' button below the table of ranges.

### Module settings

Islandora IP Embargo settings can be edited in a form located at `yoursite.com/admin/islandora/tools/ip_embargo/misc`. This page allows you to set a custom redirect page for embargoed objects, set the number of days before an embargo alert, set the overlay text for embargoed items, and set the colour of the overlay text on embargoed items.

## Usage

### Embargoing objects

1. Go to the object you want to embargo; this object can also be a collection.
2. Click the 'IP Embargo' tab, fill out the form, and click 'Set Embargo'.
3. Now this object will appear in the [managing embargoed objects form](#manage-embargoed-objects)

### Manage embargoed objects

Go to `yoursite.com/admin/islandora/tools/ip_embargo/manage`. From there, you can select objects and click the 'Delete' button to remove their embargo, or you can use this page to find embargoed objects you want to edit or examine by clicking on the desired object in the table.

## Maintainers/Sponsors

Current maintainers:

* [William Panting](https://github.com/willtp87)
* [discoverygarden](https://github.com/discoverygarden)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
