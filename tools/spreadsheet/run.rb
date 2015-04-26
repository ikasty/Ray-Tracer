#!/usr/bin/env ruby


require './spreadsheet.rb'


objs = Dir.glob("#{$OBJ}/*.obj").select{ |e| File.file? e }

p "-------read obj log files"
p objs

objs.each { |obj| 
 p "run #{obj} ------"
 `#{$BIN} -a nlogn -c #{$COUNT} -s #{$SCALE} -f #{obj} > ./#{obj}.nlogn`
 p "#{obj} nlogn"
 `#{$BIN} -a nlog2n -c #{$COUNT} -s #{$SCALE} -f #{obj} > ./#{obj}.nlog2n`
 p "#{obj} nlog2n"
} 

result = getresult(objs)
makesheet(objs, result)
