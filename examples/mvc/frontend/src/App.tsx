import {
  createBrowserRouter,
  RouterProvider,
} from 'react-router-dom'

import Layout from './layouts/Base/index'
import BlogList from './pages/Blog/List/index'
import Article from './pages/Blog/Article/index'

const router = createBrowserRouter([
  {
    path: '/',
    Component: Layout,
    children: [
      {
        path: '/',
        Component: BlogList,
      },
    ],
  },
  {
    path: '/article/',
    Component: Layout,
    children: [
      {
        path: ':id',
        Component: Article,
      },
    ],
  },
])

export default function App() {
  return <RouterProvider router={router} fallbackElement={<p>Loading...</p>} />
}
