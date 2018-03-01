# OneProfile

Run this project:

```
npm install
npm start
# in another tab
npm run webpack
# in *another* tab if you don't want the chrome extension version
npx serve
```

In Chrome, you can add an unpacked extension, just add the root
director (with the `manifest.json` file) as the extension. You should
get a icon added to your toolbar that'll work on youtube.

Otherwise, just visit the npx url and it'll show you the related talks
to the id set in `index.re`
