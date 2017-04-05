#Implementation Documentation

This page gives some detail arguments about the implementation. Some
implementation notes are included in the beginning of implementation files
(`cpp` files).

## Feature 1

*List the top 10 most active host/IP addresses that have accessed the site.*

### Offline Algorithm

Offline algorithm is straightforward: use a hashmap (maps from host to number of
connection) to record how many times each host appears in the log file, and
after processing all log events, dump the content in hashmap to a vector and
partially sort the vector to get the top 10 most active host.

The hashmap gives a O(1) access time so reading all entries in the log file
results in O(N) time complexity where N is the number of entries in the log
file. The time complexity of partial sort is O(M\*log(10)) where M is the number
of unique hosts. So the overall time complexity is O(N + M\*log(10)) = O(N).

The space complexity of hashmap and the vector is the same O(M), so the overall
space complexity is O(M).

### Online Algorithm

It is realized that by reading one more line in the log file only one host need
to be processed and the number of connection from the host only increments by 1,
so a full sorting is not necessary. We still need a hashmap for recording, but
we can use a vector of size 10 to keep track of the top 10 most active hosts so
far. Processing one line in the log file results in bubbling up at most one host
in the vector.

The time complexity associated with hashmap is again O(N). The time complexity
of bubbling up an element in the vector is O(10). So the overall time complexity
is O(N).

The space complexity is simply O(M + 10) = O(M).

One note before we move on to next topic, an alternative to a vector is a binary
search tree and the time complexity of changing an element (deleting the old and
inserting the new element) is O(2\*log(10)). But 10 is a relatively small number
such that using a bubbling algorithm with vector is sufficient.

### Offline vs. Online

While the implementations for the two classes of algorithm are provided, I
choose to use the offline algorithm in practice. The reason is that the
statistics of top 10 most active host is usually computed for a specific time
span, such as one day, one week or one month, and we can compare the statistics
from different time periods. In such a scenario, we are usually looking at
offline data rather than real-time data. That being said, if we want more
granular statistics within a time span, such as the statistics for every hour in
one day's data, the online algorithm will have some edge.

## Feature 2

*Identify the 10 resources that consume the most bandwidth on the site*

This is very similar to [Feature 1](#feature-1). Instead of keeping track of the
number of each host accessing the server, we need to keep track of the number of
byte sent over network for each URL. So the algorithms and the arguments are the
same.

## Feature 3

*List the top 10 busiest (or most frequently visited) 60-minute periods*

### Offline Algorithm

The key observation is that the log file is chronological, so when a new line of
log is processed, time points that are more than 1 hour earlier than the newly
processed time point is in the finalized 1-hour periods while the time points
that are within 1 hour of the newly processed time point are subject to change
if there are still lines of log to be processed. So I use a min-heap to keep
track of the top 10 busiest periods that are finalized and use a queue to keep
track of periods that are still active (subject to change). One caveat is that
because the period can begin at any time point but not limited to the time point
of events, so when finalizing periods from active queue to the min-heap I need
to consider every seconds that are finalized. Another caveat is that the period
cannot begin before the time point of first event in the log.

For this offline algorithm I will dump all periods in the active queue into the
min-heap at the end when statistics are needed. This destroys the invariant so I
cannot process more log events after this step.

The time complexity of the algorithm is O(T\*log(10)) where T is the number of
seconds from the first event to the last event in the log file, because we need
to consider every second as the beginning of a period and the min-heap has a
time complexity of O(log(10)) for modification.

The space complexity is O(10 + M) where M is the number of events that are
within 1 hour which are kept in the active queue.

### Online Algorithm (not implemented)

An online algorithm could be very similar to the offline algorithm. Instead of
changing the min-heap and the active queue to calculate the statistics, we could
make copies and calculate the statistics with the duplicates. 

## Feature 4

*Detect patterns of three failed login attempts from the same IP address over 20
seconds so that all further attempts to the site can be blocked for 5 minutes.
Log those possible security breaches.*

### Online Algorithm

The online algorithm is nature: use a hashmap that maps from host to the
associated information struct. The information struct keeps track of how many
times each host has failed on login, whether this host is being blocked and
until when the host is blocked. Base on the information the blocked connection
can be detected and printed in real-time.

The time complexity for processing one line of log is O(1), so the overall time
complexity is O(N).

The space complexity is O(M) where M is the number of unique host.

One possible approach to reduce the long term memory footprint is to
periodically purge some entries in the hashmap that are not actively blocked and
have time stamps long before the current time. To improve upon this approach, we
could keep track of the frequency of being blocked for each host and prioritize
purging of those hosts that are rarely blocked.
