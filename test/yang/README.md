Test syslog
===========

The base module is **ietf-syslog** with deviation that remove **tls** and
disable features **signed-messages**, **select-match** and
**select-adv-compare**.

```sh
yanglint -f tree test/yang/modules/ietf-syslog.yang -p test/yang/modules/ -F "ietf-syslog:console-action,structured-data,file-limit-size,file-limit-duration,remote-source-interface,file-action,remote-action" test/yang/dpack-syslog-deviation.yang
```

The tree resulting
------------------

```
module: ietf-syslog
  +--rw syslog!
     +--rw actions
        +--rw console! {console-action}?
        |  +--rw facility-filter
        |     +--rw facility-list* [facility severity]
        |        +--rw facility    union
        |        +--rw severity    union
        +--rw file {file-action}?
        |  +--rw log-file* [name]
        |     +--rw name               inet:uri
        |     +--rw facility-filter
        |     |  +--rw facility-list* [facility severity]
        |     |     +--rw facility    union
        |     |     +--rw severity    union
        |     +--rw structured-data?   boolean {structured-data}?
        |     +--rw file-rotation
        |        +--rw number-of-files?   uint32 {file-limit-size}?
        |        +--rw max-file-size?     uint32 {file-limit-size}?
        |        +--rw rollover?          uint32 {file-limit-duration}?
        |        +--rw retention?         uint32 {file-limit-duration}?
        +--rw remote {remote-action}?
           +--rw destination* [name]
              +--rw name                 string
              +--rw (transport)
              |  +--:(udp)
              |     +--rw udp
              |        +--rw address?   inet:host
              |        +--rw port?      inet:port-number
              +--rw facility-filter
              |  +--rw facility-list* [facility severity]
              |     +--rw facility    union
              |     +--rw severity    union
              +--rw structured-data?     boolean {structured-data}?
              +--rw facility-override?   identityref
              +--rw source-interface?    if:interface-ref {remote-source-interface}?
```