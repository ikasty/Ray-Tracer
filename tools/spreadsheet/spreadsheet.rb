#!/usr/bin/env ruby

require 'spreadsheet'
require './parse.rb'




def makesheet(objs,result)

p "----make spreadsheet"
book = Spreadsheet::Workbook.new
objs.each { |obj|
 a = obj+".nlogn"
 b = obj+".nlog2n"
 name = obj.split('/')[2]
 sheet = book.create_worksheet
 sheet.name = name
 r1 = result[a].first
 r2 = result[b].first
 sheet.row(0).replace [a,r1.depth,r1.cost,r1.below,r1.above,b,r2.depth,r2.cost,r2.below,r2.above]
 result[a].shift
 result[b].shift
 sheet.row(1).replace ['frame','depth','costs','below','above','frame','depth','costs','below','above']
 result[a].each_with_index { |data, i|
#  unless data.frame < 0 then
   sheet.row(i+2).push data.frame
   sheet.row(i+2).push data.depth.to_i
   sheet.row(i+2).push data.cost.to_f
   sheet.row(i+2).push data.below.to_i
   sheet.row(i+2).push data.above.to_i
#  end
 }
 result[b].each_with_index { |data, i|
#  unless data.frame < 0 then
   sheet.row(i+2).push '','','','','' if sheet.row(i+2).size < 1
   sheet.row(i+2).push data.frame
   sheet.row(i+2).push data.depth.to_i
   sheet.row(i+2).push data.cost.to_f
   sheet.row(i+2).push data.below.to_i
   sheet.row(i+2).push data.above.to_i
#  end
 }

}
book.write 'result.xls'

end

