begin
  require_relative 'lib/erb/version'
rescue LoadError
  # for Ruby core repository
  require_relative 'version'
end

Gem::Specification.new do |spec|
  spec.name          = 'erb'
  spec.version       = ERB::VERSION
  spec.authors       = ['Masatoshi SEKI', 'Takashi Kokubun']
  spec.email         = ['seki@ruby-lang.org', 'k0kubun@ruby-lang.org']

  spec.summary       = %q{An easy to use but powerful templating system for Ruby.}
  spec.description   = %q{An easy to use but powerful templating system for Ruby.}
  spec.homepage      = 'https://github.com/ruby/erb'
  spec.licenses      = ['Ruby', 'BSD-2-Clause']

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = spec.homepage
  spec.metadata['changelog_uri'] = "https://github.com/ruby/erb/blob/v#{spec.version}/NEWS.md"

  spec.files = Dir.chdir(__dir__) do
    begin
      files = IO.popen(%w[git ls-files -z], err: File::NULL, &:read)
      files = '' unless $?.success?
    rescue Errno::ENOENT
      files = ''
    end
    files = if files.empty?
      Dir.glob('**/*', File::FNM_DOTMATCH).select { |f| File.file?(f) }
    else
      files.split("\x0")
    end
    files.reject do |f|
      f.match?(%r{^(test|\.git|\.github|tmp)(?:/|$)}) ||
        f.match?(%r{(?:/Makefile|\.(?:bundle|gem|o|so))$})
    end
  end
  spec.bindir        = 'libexec'
  spec.executables   = ['erb']
  spec.require_paths = ['lib']

  spec.required_ruby_version = '>= 3.2.0'

  if RUBY_ENGINE == 'jruby'
    spec.platform = 'java'
  else
    spec.extensions = ['ext/erb/escape/extconf.rb']
  end
end
