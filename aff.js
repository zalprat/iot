new Vue({
    el: '#app',
    data: {
      sensorData: [
        {
          suhu: 0,
          humidi: 0,
          suhurata: 0,
          nilai_suhu_max_humid_max: 0
        },
        {
          index: 0,
          suhu: 0,
          humidi: 0,
          kecerahan: 0,
          timestamp: ''
        },
        {
          index: 0,
          suhu: 0,
          humidi: 0,
          kecerahan: 0,
          timestamp: ''
        }
      ]
    },
    methods: {
      fetchData() {
        // Data JSON dari backend
        const jsonData = [
          {
            suhu: 36,
            humidi: 73,
            suhurata: 28.35,
            nilai_suhu_max_humid_max: 36
          },
          {
            index: 1,
            suhu: 56,
            humidi: 98,
            kecerahan: 27,
            timestamp: "2019-08-28 07:23:45"
          },
          {
            index: 226,
            suhu: 56,
            humidi: 27,
            kecerahan: 56,
            timestamp: "2015-05-01 12:39:04"
          }
        ];
  
        // Parsing dan menyimpan data ke dalam sensorData
        this.sensorData = jsonData;
      }
    },
    mounted() {
      this.fetchData(); // Ambil data saat aplikasi dimuat
    }
  });
  