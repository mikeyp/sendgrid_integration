SendGrid Integration for Drupal
--------------------------------------------------------------------------------

This project is not affiliated with SendGrid, Inc.
Use the issue tracker for bug reports or questions about Drupal integration.
If you want more info about SendGrid email services, contact SendGrid
(https://sendgrid.com).

FUNCTIONALITY
--------------------------------------------------------------------------------
This module overrides default email sending behaviour, 
sending emails throught SendGrid services instead.

REQUIREMENTS
--------------------------------------------------------------------------------
Module dependencies:
Composer Manager - A Drupal module to faciltate the use of Composer:
https://getcomposer.org
Composer is being used because it is time to get off the island:
https://www.acquia.com/blog/using-composer-manager-get-island-now

Mailsystem - A module to create an agnostic management layer for Mail. Very
useful for controling the mail system on Drupal.

INSTALLATION
--------------------------------------------------------------------------------
Installing this module is simple

1. Move this folder under modules directory of your installation,
   example sites/all/modules or sites/default/modules
   
2. Navigate to Modules and enable SendGrid Integration in the Mail category.

3. Configure your SendGrid Username and API-Key in admin/config/system/sendgrid

OPTIONAL
--------------------------------------------------------------------------------

If sending email fails with certain (pre-defined) response codes the message be
added to Cron Queue for later delivery. In order for this to function, you must
configure Cron running period and when it is possible also add your drupal site
to crontab (Linux only), read more about cron at https://www.drupal.org/cron.
