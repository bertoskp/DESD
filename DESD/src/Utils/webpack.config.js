const HtmlWebPackPlugin = require('html-webpack-plugin');
module.exports = {
    devServer: {
      historyApiFallback: true,
    },
    output: {
      publicPath: '/',
    },
    devServer: {
      contentBase: './dist'
    },
    module: {
      rules: [{
          test: /\.js$/,
          exclude: /node_modules/,
          use: {
            loader: 'babel-loader'
          }
        },
        {
          test: /\.html$/,
          use: [{
            loader: 'html-loader'
          }]
        },
        {
          test: /\.css$/,
          use: [{
              loader: 'style-loader'
            },
            {
              loader: 'css-loader'
            }
          ]
        },
        {
          test: /\.(png|jpg)$/,
          loader: 'url-loader?limit=8192'
        },
        {
          test: /\.styl$/,
          loader: 'css-loader!stylus-loader?paths=node_modules/bootstrap-stylus/stylus/'
        },
        {
          test: /\.ttf$/,
          loader: 'file-loader?name=fonts/[name].[ext]'
        }
      ]
    },
    plugins: [
      new HtmlWebPackPlugin({
        template: './src/index.html',
        filename: 'index.html'
      })
    ]
};       