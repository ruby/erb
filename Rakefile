require 'bundler/gem_tasks'
require 'rake/testtask'

Rake::TestTask.new(:test) do |t|
  t.libs << 'test/lib'
  t.ruby_opts << '-rhelper'
  t.test_files = FileList['test/**/test_*.rb']
end

if RUBY_ENGINE != 'jruby'
  require 'rake/extensiontask'
  Rake::ExtensionTask.new('erb2')
  task test: :compile
end

task :sync_tool do
  require 'fileutils'
  FileUtils.cp '../ruby/tool/lib/core_assertions.rb', './test/lib'
  FileUtils.cp '../ruby/tool/lib/envutil.rb', './test/lib'
  FileUtils.cp '../ruby/tool/lib/find_executable.rb', './test/lib'
end

task default: :test
