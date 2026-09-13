# frozen_string_literal: true

require 'benchmark/ips'
require 'fileutils'
require 'erb'

DATA_DIR = "/tmp/erb-bench"

BRANCH ||= begin
  branch = `git rev-parse --abbrev-ref HEAD`.strip
  branch = `git rev-parse HEAD`.strip if branch == "HEAD"
  branch
end

def bench(name, string)
  puts "== #{name} =="
  Benchmark.ips do |x|
    x.report(BRANCH) { ERB::Util.html_escape(string) }
    FileUtils.mkdir_p(DATA_DIR)
    x.compare!(order: :baseline)
    x.save!(File.join(DATA_DIR, "#{name.tr(" ", "-")}.data"))
  end
  puts
end

bench("1k no matches", ("a" * 1024).freeze)
bench("1k few matches", (("a" * 127 + "<") * 8).freeze)
bench("1k many matches", (("a" * 31 + "<") * 32).freeze)
