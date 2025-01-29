Proof of concept WMI virus. Does what it looks like it does. Virus isn't stored on the filsystem (in any way an AV would detect), but within the WMI. Contains PoC code for extracting it from the WMI- which can also be achieved at boot from within the WMI itself using powershell. So, self-extracting WMI virus that never touches the disk.

Main WMI stuff for reading/writing files to the WMI:

[https://raw.githubusercontent.com/pulpocaminante/Stuxnet/refs/heads/main/WMIFSInterface.hpp](https://github.com/pulpocaminante/Stuxnet/blob/main/WMIFSInterface.hpp) 

[https://github.com/pulpocaminante/Stuxnet/blob/main/wmi.h](https://github.com/pulpocaminante/Stuxnet/blob/main/wmi.h)

Of particular interest to people wanting to understand how it reads/writes file, my main debugging function:

[https://github.com/pulpocaminante/Stuxnet/blob/main/wmi.h#L324](https://github.com/pulpocaminante/Stuxnet/blob/main/wmi.h#L324)

WMI based race condition 0day for demoting protected anti-malware services to less than a guest user:

[https://github.com/pulpocaminante/Stuxnet/blob/main/AntiAV.hpp](https://github.com/pulpocaminante/Stuxnet/blob/main/AntiAV.hpp)

For those who are unfamiliar:

The WMI is an extension of the Windows Driver Model. It's a CIM interface that provides all kinds of information about the system hardware, and provides for a lot of the core functionality in Windows. For example, when you create a startup registry key for an an application, that's really acting on the WMI at boot.

You can use the WMI to start applications directly. This is a known technique and antiviruses already detect it. The WMI stores triggers for events, among other things. Its a kind of database, which is accessed using a more cursed version of SQL called WQL.

So... you can write small amounts of data to it. So... I figured why not go a step further and use the WMI as a filsystem?

You can write the binary payload to the WMI, and then create a startup entry that stores a powershell script which then extracts the binary from the WMI and loads the whole program into memory. Bam. The virus never touches the disk.

Contains a novel privilege escalation technique and some other fun and/or novel stuff. Fully undetected by all antiviruses and sandboxing suites like virustotal. Loads system libraries on-demand, finds function offsets for its hardcoded prototypes, that way all of its system API calls are undetectable. Maybe some more novel AV evasion stuff, its been a while since I wrote it. Think I implemented polymorphism or dynamic runtime string encryption. Has wrappers for system libraries for AV evasion. If I recall correctly I used the "stolen bytes" technique for some of them but I don't care to look. If you care enough you'll figure it out anyway.

As a side note, and probably a free $100k for a bounty hunter:

The WMI has no buffer overflow protection for key/value pairs. Its also directly accessed by the kernel. And WMI buffer overflows can cause very strange system behavior when that data is malformed. Its my gut feeling that this could be leveraged to access kernelspace and load an unsigned device driver. But I've never gotten around to investigating it. 
