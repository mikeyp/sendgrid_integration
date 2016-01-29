SendGrid Integration for Drupal
--------------------------------------------------------------------------------
This project is not affiliated with SendGrid, Inc.

Use the issue tracker located at Drupal.org
(https://www.drupal.org/sendgrid_integration) for bug reports or questions
about this module.
If you want more info about SendGrid services, contact SendGrid
(https://sendgrid.com).

FUNCTIONALITY
--------------------------------------------------------------------------------
This module overrides default email sending behaviour andsending emails through
SendGrid Transactional Email service instead.

REQUIREMENTS
--------------------------------------------------------------------------------
Module dependencies:
Composer Manager (https://www.drupal.org/project/composer_manager) - A Drupal
module to faciltate the use of Composer (https://getcomposer.org) on a per-module
basis.

Composer is the definitive source for PHP dependencies, but implementing
Composer in modules is difficult to say the least. Composer typically acts over
an entire project - in this case it would be your entire Drupal source code. 
Composer manager allows for modules to declare their own composer.json file
rather than a project global composer.json.
  
  Composer is being used because of the usefulness it offers:
  https://www.acquia.com/blog/using-composer-manager-get-island-now

Running Composer and composer manager will require command line access via
DRUSH. There is no GUI for this tool.

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

If you would like a record of the emails being sent by the website, installing
Maillog (https://www.drupal.org/project/maillog) will allow you to store local
copies of the emails sent. Sendgrid does not store the content of the email.

RESOURCES
--------------------------------------------------------------------------------
Information about the Sendgrid PHP Library is availabe on Github:
https://github.com/sendgrid/sendgrid-php