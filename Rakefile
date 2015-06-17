require "bundler/gem_tasks"
require 'rake/extensiontask'
require 'rspec/core/rake_task'

gem_spec = Gem::Specification.load('lazer_lines.gemspec')
Rake::ExtensionTask.new('lazer_lines', gem_spec) do |ext|
  ext.lib_dir = 'lib/lazer_lines'
end

RSpec::Core::RakeTask.new('spec')

task :default => [:compile, :spec]

namespace :lazer_lines do
  desc 'Benchmark lazer_lines gem against polylines gem'
  task :benchmark do
    require 'lazer_lines'
    require 'polylines'
    require 'benchmark'

    TEST_REPS = 50

    last_point = [0.0, 0.0]

    test_points = Array.new(2000) do |idx|
      [last_point[0] += ((0.5 - rand()) * 5), last_point[1] += ((0.5 - rand()) * 5)]
    end
    test_polyline = Polylines::Encoder.encode_points(test_points)

    puts "\nEncoding #{test_points.length} points #{TEST_REPS} times:"
    Benchmark.bm(10) do |x|
      x.report('Polylines:') { TEST_REPS.times { Polylines::Encoder.encode_points(test_points) } }
      x.report('LazerLines:') { TEST_REPS.times { LazerLines.encode(test_points) } }
    end

    puts "\nDecoding #{test_polyline.length}-character polyline #{TEST_REPS} times:"
    Benchmark.bm(10) do |x|
      x.report('Polylines:') { TEST_REPS.times { Polylines::Decoder.decode_polyline(test_polyline) } }
      x.report('LazerLines:') { TEST_REPS.times { LazerLines.decode(test_polyline) } }
    end
  end

  desc 'Create random point strings, looking for encoding/decoding failures'
  task :find_failures do
    require 'lazer_lines'
    require 'polylines'

    1000.times do |i|
      last_point = [0.0, 0.0]

      line_string = Array.new(2000) do |idx|
        [last_point[0] += ((0.5 - rand()) * 5), last_point[1] += ((0.5 - rand()) * 5)]
      end

      encoded_polyline = Polylines::Encoder.encode_points(line_string)
      encoding_result = LazerLines.encode(line_string)

      if encoding_result != encoded_polyline
        puts "Test #{i}: Result #{encoding_result} does not match expected #{encoded_polyline} for #{line_string}"
        break
      end

      orig_decoded_points = Polylines::Decoder.decode_polyline(encoded_polyline)
      decoded_points = orig_decoded_points.map { |ls| ls.map { |pt| pt.round(10) } }

      orig_decoding_result = LazerLines.decode(encoded_polyline)
      decoding_result = orig_decoding_result.map { |ls| ls.map { |pt| pt.round(10) } }

      if decoding_result != decoded_points
        idx_point = decoding_result.index { |pt| pt != decoded_points[decoding_result.index(pt)] }

        puts "Previous point should have been #{decoding_result[idx_point - 1]}" if idx_point > 0
        puts "Expected #{decoded_points[idx_point]}, got #{decoding_result[idx_point]}"
        puts "Original points were #{line_string[(idx_point - 1)..(idx_point)]}"
        break
      end
    end
  end
end
