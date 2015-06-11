require "lazer_lines/version"

module LazerLines
  # Your code goes here...
  def self.encode(points, precision_digits = 5)
    encode_with_precision(points, precision_digits)
  end

  def self.decode(polyline, precision_digits = 5)
    decode_with_precision(polyline, precision_digits)
  end
end

require "lazer_lines/lazer_lines"
