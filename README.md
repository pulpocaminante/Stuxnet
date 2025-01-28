Proof of concept WMI virus. Does what it looks like it does.

Contains a novel privilege escalation technique and some other fun and/or novel stuff. Fully undetected by all antiviruses and sandboxing suites like virustotal. Loads system libraries on-demand, finds function offsets for hardcoded prototypes, that way all of its system API calls are undetectable. Maybe some more novel AV evasion stuff, its been a while since I wrote it. Think I implemented polymorphism or dynamic runtime string encryption.

It turns out that most heuristic suites like hybrid analysis or virustotal will regard a file as clean if it bluescreens their VM. Has wrappers for system libraries for AV evasion. If I recall correctly I used the "stolen bytes" technique for some of them but I don't care to look. If you care enough you'll figure it out anyway.
