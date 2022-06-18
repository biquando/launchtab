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
- System launchtabs are not supported. A user can only modify their own config.
