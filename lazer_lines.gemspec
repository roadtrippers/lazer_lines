# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'lazer_lines/version'

Gem::Specification.new do |spec|
  spec.name          = "lazer_lines"
  spec.version       = LazerLines::VERSION
  spec.authors       = ["Eric Lutley"]
  spec.email         = ["eric.lutley@roadtrippers.com"]

  spec.summary       = "Ruby gem for fast encoding and decoding of Google polylines."
  # spec.description   = %q{TODO: Write a longer description or delete this line.}
  spec.homepage      = "https://github.com/roadtrippers/lazer_lines"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions << "ext/lazer_lines/extconf.rb"

  spec.add_development_dependency "bundler"#, "~> 1.8"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rake-compiler", "~> 0.9"
  spec.add_development_dependency "rspec"
  spec.add_development_dependency "rspec_junit_formatter"
  spec.add_development_dependency "polylines"
end
