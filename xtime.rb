#!/usr/bin/env ruby
require "mkmf"
require "socket"

def mem(pid)
  parent_rss = `ps p #{pid} -o rss`
  overall = parent_rss.split("\n").last.to_i
  unless RUBY_PLATFORM =~ /darwin/
    children_rss = `ps --ppid #{pid} -o rss`
    children_rss.split("\n").drop(1).each do |mem|
      overall += mem.to_i
    end
  end
  overall
end

has_energy_metrics = find_executable 'rapl-info'

def energy
  0.000001 * `rapl-info -j`.to_i
end

pid = Process.spawn(*ARGV.to_a)

server = TCPServer.new 9001
client = server.accept
puts client.gets

t = Process.clock_gettime(Process::CLOCK_MONOTONIC)
mm = 0
if has_energy_metrics
  e = energy
end

Thread.new do
  mm = mem(pid)
  while true
    sleep 0.1
    m = mem(pid)
    mm = m if m > mm
  end
end

Process.waitpid(pid, 0)
stats = "%.2f s, %.1f Mb" % [Process.clock_gettime(Process::CLOCK_MONOTONIC) - t, mm / 1024.0]
if has_energy_metrics
  stats += ", %.1f J" % [energy - e]
end
STDERR.puts stats
exit($?.exitstatus || 0)
