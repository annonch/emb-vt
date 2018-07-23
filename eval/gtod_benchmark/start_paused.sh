#!/bin/bash
kill -STOP $$
exec "$@"
