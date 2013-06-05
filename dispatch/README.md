dispatch
========

Example application to dispatch binary messages. The message format assumes
that the first byte contains an id of the messages type.

The code in written in C++, but for use in embedded systems where there is
no support for templates. Macros are used to construct message classes.

A subject / observer system allows the decoded message 
to be sent to N observers.
