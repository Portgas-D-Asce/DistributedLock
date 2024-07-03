if redis.call('hexists', KEYS[1], ARGV[1]) == 0 then
    return nil
if redis.call('hincrby', KEYS[1], ARGV[1], -1) == 0 then
    return redis.call('del', KEYS[1])
else
    return 0
end
