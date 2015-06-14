require 'spec_helper'
require 'polylines'

describe LazerLines do
  PROBLEM_POLYLINES = [
    {
      initial_points: [[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]],
      encoded_polyline: '_p~iF~ps|U_ulLnnqC_mqNvxq`@'
    },
    {
      initial_points: [[42.3611064554188, -81.23972944825465], [44.363113644895094, -77.89138888922021]],
      encoded_polyline: '}spaGhcznNo_fKc~lS'
    },
    {
      initial_points: [[40.2795240863896, -81.71530027290828], [44.38551897276031, -76.87129782851022]],
      encoded_polyline: '_bztFr_wqNo}`X_bq\\'
    },
    {
      initial_points: [[40.53964832265767, -84.25236968077888], [40.729113589809046, -84.23348518048891]],
      encoded_polyline: 'y{lvFhpfaOc_d@_uB'
    },
    {
      initial_points: [[39.922478487732654, -81.25999262105793], [41.78520998914982, -78.60175609553673]],
      encoded_polyline: 'ojtrF|a~nNayjJ}dfO'
    },
    {
      initial_points: [[-28.456746777739394, -63.97375403734292], [-30.17150593261877, -62.48803316786342]],
      encoded_polyline: 't}tlD|zmeKf|mIwdaH'
    },
    {
      initial_points: [[-68.56664124937527, 126.22230917872295], [-70.68129066799365, 128.2193652532957]],
      encoded_polyline: 'n|naLmxkaWp_|Ks`eK'
    },
    {
      initial_points: [[30.799351812954576, 87.5984661481003], [33.277776842221705, 89.04301272072743]],
      encoded_polyline: '}n~zDmatuOeacNkcyG'
    }
  ]

  PROBLEM_POLYLINES.each do |data|
    describe ".encoding #{data[:initial_points]}" do
      subject { LazerLines.encode(data[:initial_points]) }

      it { should == data[:encoded_polyline] }
    end

    describe ".decoding #{data[:encoded_polyline]}" do
      let(:decoded_points) { data[:initial_points].map { |pt| pt.map { |n| n.round(5) } } }
      subject { LazerLines.decode(data[:encoded_polyline]) }

      it { should == decoded_points }
    end
  end

  context 'heading north-east' do
    let(:initial_points) { [[41.87314, -87.74471], [41.91148, -87.71004]] }
    let(:encoded_polyline) { 'cjq~FlspvOsnFuwE' }
    let(:decoded_polyline) { [[41.87314, -87.74471], [41.91148, -87.71004]] }

    describe '.encode' do
      subject { LazerLines.encode(initial_points) }
      it { should == encoded_polyline }
    end

    describe '.decode' do
      subject { LazerLines.decode(encoded_polyline) }
      it { should == decoded_polyline }
    end
  end

  context 'heading south-east' do
    let(:initial_points) { [[41.87314, -87.74471], [41.85601, -87.71673]] }
    let(:encoded_polyline) { 'cjq~FlspvO`jB{mD' }
    let(:decoded_polyline) { Polylines::Decoder.decode_polyline(encoded_polyline) }

    describe '.encode' do
      subject { LazerLines.encode(initial_points) }
      it { should == encoded_polyline }
    end

    describe '.decode' do
      subject { LazerLines.decode(encoded_polyline) }
      it { should == decoded_polyline }
    end
  end

  context 'heading south-west' do
    let(:initial_points) { [[41.87314, -87.74471], [41.85869, -87.77218]] }
    let(:encoded_polyline) { 'cjq~FlspvOhyAtjD' }
    let(:decoded_polyline) { [[41.87314, -87.74471], [41.85869, -87.77218]] }

    describe '.encode' do
      subject { LazerLines.encode(initial_points) }
      it { should == encoded_polyline }
    end

    describe '.decode' do
      subject { LazerLines.decode(encoded_polyline) }
      it { should == decoded_polyline }
    end
  end

  context 'heading north-west' do
    let(:initial_points) { [[41.87314, -87.74471], [41.89039, -87.76857]] }
    let(:encoded_polyline) { 'cjq~FlspvOyjBbtC' }
    let(:decoded_polyline) { Polylines::Decoder.decode_polyline(encoded_polyline) }

    describe '.encode' do
      subject { LazerLines.encode(initial_points) }
      it { should == encoded_polyline }
    end

    describe '.decode' do
      subject { LazerLines.decode(encoded_polyline) }
      it { should == decoded_polyline }
    end
  end

  describe 'variable-precision support' do
    let(:initial_points) { [[-68.56664124937527, 126.22230917872295], [-70.68129066799365, 128.2193652532957]] }
    let(:polyline_6_digits) { "`f~waCi}~voFrda`C_o{xB" }
    let(:decoded_6_digit_points) { [[-68.566641, 126.222309], [-70.681291, 128.219365]] }

    describe 'encoding with 6 digits of precision' do
      subject { LazerLines.encode(initial_points, 6) }
      it { should == polyline_6_digits }
    end

    describe 'decoding with 6 digits of precision' do
      subject { LazerLines.decode(polyline_6_digits, 6) }
      it { should == decoded_6_digit_points }
    end

    context 'with 8 digits of precision' do
      it 'encoding raises an ArgumentError' do
        expect{ LazerLines.encode(initial_points, 8) }.to raise_error(RangeError)
      end

      it 'decoding raises an ArgumentError' do
        expect{ LazerLines.decode(polyline_6_digits, 8) }.to raise_error(RangeError)
      end
    end
  end
end
