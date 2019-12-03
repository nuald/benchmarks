require 'json'
require 'socket'

begin
  Socket.tcp('localhost', 9001) { |s|
    s.puts "#{RUBY_ENGINE}"
  }
rescue
  # standalone usage
end

jobj = JSON.parse(File.read('1.json'))
coordinates = jobj['coordinates']
len = coordinates.length
x = y = z = 0

coordinates.each do |coord|
  x += coord['x']
  y += coord['y']
  z += coord['z']
end

p x / len
p y / len
p z / len
