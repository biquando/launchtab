# Launchtab
Like crontab, but for launchd.

## What's wrong with crontab?
In macOS, cron has been deprecated in favor of launchd as a scheduling daemon.
launchd jobs are much more versatile than cron jobs. Notably, launchd jobs that
are scheduled to run when the computer is asleep will be run the next time the
computer wakes up (see `launchd.plist(5)`). Cron jobs that run when the computer
is asleep are lost.

Launchtab is a tool to create launchd jobs just like you would create cron jobs
with crontab. It also lets you easily configure and manage your more complex
tasks.

## Building and Installation
1. Clone the repository.
```
git clone 'https://github.com/biquando/launchtab.git'
```
2. Build the executable.
```
cd launchtab
make
```
3. Move the executable to your PATH.
```
# For example:
mv bin/launchtab ~/bin
```

## Making a launchtab config
If you already have a crontab, you can import it with `crontab -l | launchtab`.
You can also edit your launchtab directly with `launchtab -e`. See `examples/`
for some example launchtabs. Run `launchtab --help` for usage information.

LaunchTab supports most features of crontab. Currently not supported:
- Passing text to stdin via `%`
- `@AppleNotOnBattery` prefix
- `@reboot` special calendar entry
- System-wide configuration. A user can only modify their own config.
