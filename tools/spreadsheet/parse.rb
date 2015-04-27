#!/usr/bin/env ruby
require './environment'


NData = Struct.new(:frame, :depth, :cost, :below, :above)



#unused method
def getline(x,data)
 str = x.split('buildTree')
    
 t = str[1].split(',')
 t.each { |x|
  x.split!(' ')
  data[x[0]] = x[1]
 }

 str[0].include? 'frame'
end
#ddddd
#
#
#



def getfile(x)
 
 count = 1
 result = Array.new
 File.foreach(x) { |line|
  if ((line.include? 'DEBUG]') && (line.include? 'frame')) then 
   count += 1
  end
  str = line.split('depth ', 2)
  if str.size >= 2 then 
   data = NData.new
   data.frame = count
   t = str[1].split(',', 2)[0]
   #p t
   data.depth = t
   str = str[1]
   str = str.split('cost ', 2)
   t = str[1].split(',', 2)[0]
   #p t
   data.cost = t
   str = str[1].split('below ', 2)
   t = str[1].split(',', 2)[0]
   #p t
   data.below = t
   str = str[1].split('above ', 2)
   t = str[1]
   #p t
   data.above = t
   result.push data
  else
   if str[0][0..4] == "build" then
    str[0].delete!("\n")
    t = str[0].split(', ')
    data = NData.new(-1, t[0], t[1], t[2], t[3])
    result.unshift data
   end
  end
 }

 result
end


def getresult(objs)
 p "----make Hash "
 result = Hash.new
 objs.each { |obj|
  a = obj+".nlogn"
  b = obj+".nlog2n" 
  result[a] = getfile(a)
  p a
  result[b] = getfile(b)
  p b
 }
 result
end

  
