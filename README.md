# LaunchTab
Like crontab, but for launchd.

## Building
Run `make` to build. The executable will be `bin/launchtab`. You can move this
to anywhere on your PATH.

## Making a LaunchTab Config
If you already have a crontab, you can port it directly with `launchtab <file>`.
You can also edit your launchtab directly with `launchtab -e`. See `example.tab`
for an example launchtab. Run `launchtab --help` for usage information.

LaunchTab supports most features of crontab. Currently not supported:
- `%` interpreted as newline
- `@AppleNotOnBattery` prefix
- `@reboot` special calendar entry

## Limitations
- The plists are not automatically loaded/unloaded, so you need to do this
	manually or logout and log back in for your changes to take effect. I've
	included a shell script `bin/ltreload` to help you reload your plists.
