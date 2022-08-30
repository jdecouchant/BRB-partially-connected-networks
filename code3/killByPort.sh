#!/bin/bash

kill $(lsof -ti tcp:$1)
