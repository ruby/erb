require 'bundler/gem_tasks'
require 'rake/testtask'

if RUBY_ENGINE != 'jruby'
  require 'rake/extensiontask'
  extask = Rake::ExtensionTask.new('erb/escape') do |ext|
    ext.lib_dir.sub!(%r[(?=/|\z)], "/#{RUBY_VERSION}/#{ext.platform}")
  end
end

Rake::TestTask.new(:test) do |t|
  t.libs << 'test/lib'
  if extask
    t.libs << "lib/#{RUBY_VERSION}/#{extask.platform}"
  end
  t.ruby_opts << '-rhelper'
  t.test_files = FileList['test/**/test_*.rb']
end

task :sync_tool do
  require 'fileutils'
  FileUtils.cp '../ruby/tool/lib/core_assertions.rb', './test/lib'
  FileUtils.cp '../ruby/tool/lib/envutil.rb', './test/lib'
  FileUtils.cp '../ruby/tool/lib/find_executable.rb', './test/lib'
end

task default: :test
