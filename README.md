# What is Daedalus?
Daedalus is a simple in-memory cache. The cache stores data as key-value pairs, where the keys are interface types and the values can be of any type. The cache is designed to be concurrent-safe with the use of a sync.Mutex.

This library "daedalus" is a pure implementation and does not rely on any external dependencies. It is a self-contained implementation of an in-memory cache.

# API
Daedalus supports the following operations:
createDaedalus() creates a new instance of the cache. Parameters include checkFrequency (for frequency of cache eviction check)
and timeToLive (how long an entry can stay in the cache)

evict() starts the eviction process, where entries that have been in the cache for too long (or expired timeToLive) are remove

add() adds a key value pair to the cache

get() retrieves the value for a given key in the cache

The cache is periodically updated every checkFrequency interval.

Specs:

Cache size: 100 entry hashtable
Eviction policy: Time To Live
Data storage: In Memory

