require 'spec_helper'
require 'polylines'

describe LazerLines do

  describe '.flexpolyline_decode' do

    describe "HERE's test decoding test case" do
      # Derived from: https://github.com/heremaps/flexible-polyline/blob/aa961322021ef07e72d42f3602885e2218a97e1a/cpp/test/test.cpp
      TEST_CASES = [
        {
          decoded: [
            [8.69821, 50.10228],
            [8.69567, 50.10201],
            [8.69150, 50.10063],
            [8.68752, 50.09878]
          ],
          encoded: 'BFoz5xJ67i1B1B7PzIhaxL7Y'
        },
        {
          decoded: [
            [13.38663, 52.51994],
            [13.28169, 52.51009],
            [13.19352, 52.43518],
            [13.19645, 52.41073],
            [13.15578, 52.38871],
            [13.14910, 52.37278],
            [13.11546, 52.37375],
            [13.08722, 52.38752],
            [13.07062, 52.40294],
            [13.07555, 52.41058],
          ],
          encoded: 'BF05xgKuy2xCx9B7vUl0OhnR54EqSzpEl-HxjD3pBiGnyGi2CvwFsgD3nD4vB6e'
        }
      ]

      TEST_CASES.each_with_index do |test_case, index|
        describe "#{index + 1}" do
          it 'correctly decodes the polyline' do
            expect(LazerLines.flexpolyline_decode(test_case[:encoded])).to eql(test_case[:decoded])
          end
        end
      end
    end
  end
end
